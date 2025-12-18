import { Component } from '@angular/core';
import { RebalancerConfigComponent } from '@app/bots/components/rebalancer-config/rebalancer-config.component';
import { MovingAverageCrossoverConfigComponent } from '@app/bots/components/moving-average-crossover-config/moving-average-crossover-config.component';
import { DonchianChannelBreakoutStrategyConfigComponent } from '@app/bots/components/donchian-channel-breakout-strategy-config/donchian-channel-breakout-strategy-config.component';
import { CommonModule } from '@angular/common';
import { MatButtonModule } from '@angular/material/button';
import { BotName } from '@app/bots/bot-name-enum';

@Component({
  selector: 'app-bots-launch-page',
  standalone: true,
  imports: [
    CommonModule,
    MatButtonModule,
    RebalancerConfigComponent,
    MovingAverageCrossoverConfigComponent,
    DonchianChannelBreakoutStrategyConfigComponent,
  ],
  templateUrl: './bots-launch-page.component.html',
  styleUrls: ['./bots-launch-page.component.scss'],
})
export class BotsLaunchPage {
  public availableBots = [BotName.Rebalancer, BotName.MovingAverageCrossover, BotName.DonchianChannelBreakoutStrategy];
  public selectedBot = this.availableBots[0];

  public selectBot(botName: BotName): void {
    this.selectedBot = botName;
  }

  public getBotButtonClasses(botName: BotName) {
    return {
      isSelected: botName === this.selectedBot,
    };
  }

  public getDisplayName(botName: BotName): string {
    switch (botName) {
      case BotName.Rebalancer:
        return 'Rebalancer';
      case BotName.MovingAverageCrossover:
        return 'Moving average crossover';
      case BotName.DonchianChannelBreakoutStrategy:
        return 'Donchian channel breakout strategy';
      default:
        return botName;
    }
  }

  public get isRebalancerBot(): boolean {
    return this.selectedBot === BotName.Rebalancer;
  }

  public get isMovingAverageCrossoverBot(): boolean {
    return this.selectedBot === BotName.MovingAverageCrossover;
  }

  public get isDonchianChannelBreakoutStrategyBot(): boolean {
    return this.selectedBot === BotName.DonchianChannelBreakoutStrategy;
  }
}
